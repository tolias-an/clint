#include <unity/unity.h>

#include "clint/util.h"

void setUp() {}
void tearDown() {}

void test_token_list_create_and_free() {
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

void test_token_list_replace() {
    char str[] = "tokens list of tokens";
    token_list *list = token_list_create(str);

    TEST_ASSERT_EQUAL_INT(2, token_list_replace(list, "tokens", "lists"));
    TEST_ASSERT_EQUAL_INT(1, token_list_replace(list, "list", "token"));

    token_list *token = list;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("lists", token->token, 6);
    token = token->next;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("token", token->token, 5);
    token = token->next;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("of", token->token, 3);
    token = token->next;
    TEST_ASSERT_EQUAL_CHAR_ARRAY("lists", token->token, 6);

    token_list_free(list);
}

void test_token_list_bake() {
    char str[] = "A token list";
    token_list *list = token_list_create(str);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("A token list", token_list_bake(list), 13);

    token_list_free(list);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_token_list_create_and_free);
    RUN_TEST(test_token_list_replace);
    RUN_TEST(test_token_list_bake);
    return UNITY_END();
}
