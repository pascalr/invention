require 'test_helper'

class DetectedCodesControllerTest < ActionDispatch::IntegrationTest
  setup do
    @detected_code = detected_codes(:one)
  end

  test "should get index" do
    get detected_codes_url
    assert_response :success
  end

  test "should get new" do
    get new_detected_code_url
    assert_response :success
  end

  test "should create detected_code" do
    assert_difference('DetectedCode.count') do
      post detected_codes_url, params: { detected_code: { centerX: @detected_code.centerX, centerY: @detected_code.centerY, img: @detected_code.img, scale: @detected_code.scale, t: @detected_code.t, x: @detected_code.x, y: @detected_code.y } }
    end

    assert_redirected_to detected_code_url(DetectedCode.last)
  end

  test "should show detected_code" do
    get detected_code_url(@detected_code)
    assert_response :success
  end

  test "should get edit" do
    get edit_detected_code_url(@detected_code)
    assert_response :success
  end

  test "should update detected_code" do
    patch detected_code_url(@detected_code), params: { detected_code: { centerX: @detected_code.centerX, centerY: @detected_code.centerY, img: @detected_code.img, scale: @detected_code.scale, t: @detected_code.t, x: @detected_code.x, y: @detected_code.y } }
    assert_redirected_to detected_code_url(@detected_code)
  end

  test "should destroy detected_code" do
    assert_difference('DetectedCode.count', -1) do
      delete detected_code_url(@detected_code)
    end

    assert_redirected_to detected_codes_url
  end
end
