require 'test_helper'

class AlimentControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get aliment_index_url
    assert_response :success
  end

end
