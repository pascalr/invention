require "application_system_test_case"

class RecettesTest < ApplicationSystemTestCase
  setup do
    @recette = recettes(:one)
  end

  test "visiting the index" do
    visit recettes_url
    assert_selector "h1", text: "Recettes"
  end

  test "creating a Recette" do
    visit recettes_url
    click_on "New Recette"

    fill_in "Instructions", with: @recette.instructions
    fill_in "Name", with: @recette.name
    fill_in "Rating", with: @recette.rating
    click_on "Create Recette"

    assert_text "Recette was successfully created"
    click_on "Back"
  end

  test "updating a Recette" do
    visit recettes_url
    click_on "Edit", match: :first

    fill_in "Instructions", with: @recette.instructions
    fill_in "Name", with: @recette.name
    fill_in "Rating", with: @recette.rating
    click_on "Update Recette"

    assert_text "Recette was successfully updated"
    click_on "Back"
  end

  test "destroying a Recette" do
    visit recettes_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Recette was successfully destroyed"
  end
end
