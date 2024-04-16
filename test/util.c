#include "clint/util.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <unity/unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_token_list_create_and_free(void) {
    token_list *list;

    char emptry_str[] = "";
    list = token_list_create(emptry_str);
    TEST_ASSERT_NULL(list);

    char str[] = "AB CDE FGHI";
    list = token_list_create(str);

    token_list *token = list;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("AB", token->token, 3);
    token = token->next;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("CDE", token->token, 4);
    token = token->next;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("FGHI", token->token, 5);

    token_list_free(list);
}

void test_token_replace(void) {
    char str[] = "A token list";
    token_list *list = token_list_create(str);
    token_list *token = list;

    TEST_ASSERT_EQUAL_INT(0, token_replace(token, "token", "token"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("A", token->token, 2);
    token = token->next;

    TEST_ASSERT_EQUAL_INT(1, token_replace(token, "token", "token"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("token", token->token, 6);
    token = token->next;

    TEST_ASSERT_EQUAL_INT(1, token_replace(token, "list", "token"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("token", token->token, 6);
    token = token->next;

    token_list_free(list);
}

void test_token_list_bake(void) {
    char str[] = "A token list";
    token_list *list = token_list_create(str);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("A token list", token_list_bake(list), 13);

    token_list_free(list);
}

void test_path_in_env(void) {
    char str[] = "file";
    TEST_ASSERT_EQUAL_CHAR(1, path_in_env(str));

    char str_2[] = "doesnotexist";
    TEST_ASSERT_EQUAL_CHAR(0, path_in_env(str_2));
}

void test_path_canonicalise(void) {
    char base_path[] = "/a/folder";
    char *result;

    char str_1[] = "/a";
    result = path_canonicalise(base_path, str_1);
    TEST_ASSERT_NULL(result);

    char str_2[] = "file";
    result = path_canonicalise(base_path, str_2);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("/a/folder/file", result, 14);
    free(result); result = NULL;

    char str_3[] = "../file";
    result = path_canonicalise(base_path, str_3);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("/a/file", result, 7);
    free(result); result = NULL;

    char str_4[] = "../file/../folder2/file2";
    result = path_canonicalise(base_path, str_4);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("/a/folder2/file2", result, 16);
    free(result); result = NULL;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_token_list_create_and_free);
    RUN_TEST(test_token_replace);
    RUN_TEST(test_token_list_bake);
    RUN_TEST(test_path_in_env);
    RUN_TEST(test_path_canonicalise);
    return UNITY_END();
}
