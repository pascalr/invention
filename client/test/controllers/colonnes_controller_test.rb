require 'test_helper'

class ColonnesControllerTest < ActionDispatch::IntegrationTest
  setup do
    @colonne = colonnes(:one)
  end

  test "should get index" do
    get colonnes_url
    assert_response :success
  end

  test "should get new" do
    get new_colonne_url
    assert_response :success
  end

  test "should create colonne" do
    assert_difference('Colonne.count') do
      post colonnes_url, params: { colonne: { center_x: @colonne.center_x, name: @colonne.name, shelf_id: @colonne.shelf_id, width: @colonne.width } }
    end

    assert_redirected_to colonne_url(Colonne.last)
  end

  test "should show colonne" do
    get colonne_url(@colonne)
    assert_response :success
  end

  test "should get edit" do
    get edit_colonne_url(@colonne)
    assert_response :success
  end

  test "should update colonne" do
    patch colonne_url(@colonne), params: { colonne: { center_x: @colonne.center_x, name: @colonne.name, shelf_id: @colonne.shelf_id, width: @colonne.width } }
    assert_redirected_to colonne_url(@colonne)
  end

  test "should destroy colonne" do
    assert_difference('Colonne.count', -1) do
      delete colonne_url(@colonne)
    end

    assert_redirected_to colonnes_url
  end
end
