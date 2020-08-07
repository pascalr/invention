require 'test_helper'

class ConfigureControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get configure_index_url
    assert_response :success
  end

end
