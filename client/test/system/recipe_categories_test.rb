require "application_system_test_case"

class RecipeCategoriesTest < ApplicationSystemTestCase
  setup do
    @recipe_category = recipe_categories(:one)
  end

  test "visiting the index" do
    visit recipe_categories_url
    assert_selector "h1", text: "Recipe Categories"
  end

  test "creating a Recipe category" do
    visit recipe_categories_url
    click_on "New Recipe Category"

    fill_in "Name", with: @recipe_category.name
    fill_in "Recipe category", with: @recipe_category.recipe_category_id
    click_on "Create Recipe category"

    assert_text "Recipe category was successfully created"
    click_on "Back"
  end

  test "updating a Recipe category" do
    visit recipe_categories_url
    click_on "Edit", match: :first

    fill_in "Name", with: @recipe_category.name
    fill_in "Recipe category", with: @recipe_category.recipe_category_id
    click_on "Update Recipe category"

    assert_text "Recipe category was successfully updated"
    click_on "Back"
  end

  test "destroying a Recipe category" do
    visit recipe_categories_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Recipe category was successfully destroyed"
  end
end
