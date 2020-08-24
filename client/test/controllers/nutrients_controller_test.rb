require 'test_helper'

class NutrientsControllerTest < ActionDispatch::IntegrationTest
  setup do
    @nutrient = nutrients(:one)
  end

  test "should get index" do
    get nutrients_url
    assert_response :success
  end

  test "should get new" do
    get new_nutrient_url
    assert_response :success
  end

  test "should create nutrient" do
    assert_difference('Nutrient.count') do
      post nutrients_url, params: { nutrient: { name: @nutrient.name, unit_name: @nutrient.unit_name } }
    end

    assert_redirected_to nutrient_url(Nutrient.last)
  end

  test "should show nutrient" do
    get nutrient_url(@nutrient)
    assert_response :success
  end

  test "should get edit" do
    get edit_nutrient_url(@nutrient)
    assert_response :success
  end

  test "should update nutrient" do
    patch nutrient_url(@nutrient), params: { nutrient: { name: @nutrient.name, unit_name: @nutrient.unit_name } }
    assert_redirected_to nutrient_url(@nutrient)
  end

  test "should destroy nutrient" do
    assert_difference('Nutrient.count', -1) do
      delete nutrient_url(@nutrient)
    end

    assert_redirected_to nutrients_url
  end
end
