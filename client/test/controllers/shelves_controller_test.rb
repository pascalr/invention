require 'test_helper'

class ShelvesControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get shelves_index_url
    assert_response :success
  end

  test "should get new" do
    get shelves_new_url
    assert_response :success
  end

  test "should get edit" do
    get shelves_edit_url
    assert_response :success
  end

  test "should get create" do
    get shelves_create_url
    assert_response :success
  end

  test "should get update" do
    get shelves_update_url
    assert_response :success
  end

  test "should get destroy" do
    get shelves_destroy_url
    assert_response :success
  end

end
