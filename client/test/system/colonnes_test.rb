require "application_system_test_case"

class ColonnesTest < ApplicationSystemTestCase
  setup do
    @colonne = colonnes(:one)
  end

  test "visiting the index" do
    visit colonnes_url
    assert_selector "h1", text: "Colonnes"
  end

  test "creating a Colonne" do
    visit colonnes_url
    click_on "New Colonne"

    fill_in "Center x", with: @colonne.center_x
    fill_in "Name", with: @colonne.name
    fill_in "Shelf", with: @colonne.shelf_id
    fill_in "Width", with: @colonne.width
    click_on "Create Colonne"

    assert_text "Colonne was successfully created"
    click_on "Back"
  end

  test "updating a Colonne" do
    visit colonnes_url
    click_on "Edit", match: :first

    fill_in "Center x", with: @colonne.center_x
    fill_in "Name", with: @colonne.name
    fill_in "Shelf", with: @colonne.shelf_id
    fill_in "Width", with: @colonne.width
    click_on "Update Colonne"

    assert_text "Colonne was successfully updated"
    click_on "Back"
  end

  test "destroying a Colonne" do
    visit colonnes_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Colonne was successfully destroyed"
  end
end
