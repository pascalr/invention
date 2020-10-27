require 'test_helper'

class JarsControllerTest < ActionDispatch::IntegrationTest
  setup do
    @jar = jars(:one)
  end

  test "should get index" do
    get jars_url
    assert_response :success
  end

  test "should get new" do
    get new_jar_url
    assert_response :success
  end

  test "should create jar" do
    assert_difference('Jar.count') do
      post jars_url, params: { jar: { ingredient_id: @jar.ingredient_id, jar_format_id: @jar.jar_format_id } }
    end

    assert_redirected_to jar_url(Jar.last)
  end

  test "should show jar" do
    get jar_url(@jar)
    assert_response :success
  end

  test "should get edit" do
    get edit_jar_url(@jar)
    assert_response :success
  end

  test "should update jar" do
    patch jar_url(@jar), params: { jar: { ingredient_id: @jar.ingredient_id, jar_format_id: @jar.jar_format_id } }
    assert_redirected_to jar_url(@jar)
  end

  test "should destroy jar" do
    assert_difference('Jar.count', -1) do
      delete jar_url(@jar)
    end

    assert_redirected_to jars_url
  end
end
