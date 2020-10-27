require 'test_helper'

class FdcsControllerTest < ActionDispatch::IntegrationTest
  setup do
    @fdc = fdcs(:one)
  end

  test "should get index" do
    get fdcs_url
    assert_response :success
  end

  test "should get new" do
    get new_fdc_url
    assert_response :success
  end

  test "should create fdc" do
    assert_difference('Fdc.count') do
      post fdcs_url, params: { fdc: { data_type: @fdc.data_type, description: @fdc.description, food_category_id: @fdc.food_category_id, publication_date: @fdc.publication_date } }
    end

    assert_redirected_to fdc_url(Fdc.last)
  end

  test "should show fdc" do
    get fdc_url(@fdc)
    assert_response :success
  end

  test "should get edit" do
    get edit_fdc_url(@fdc)
    assert_response :success
  end

  test "should update fdc" do
    patch fdc_url(@fdc), params: { fdc: { data_type: @fdc.data_type, description: @fdc.description, food_category_id: @fdc.food_category_id, publication_date: @fdc.publication_date } }
    assert_redirected_to fdc_url(@fdc)
  end

  test "should destroy fdc" do
    assert_difference('Fdc.count', -1) do
      delete fdc_url(@fdc)
    end

    assert_redirected_to fdcs_url
  end
end
