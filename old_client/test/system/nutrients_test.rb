require "application_system_test_case"

class NutrientsTest < ApplicationSystemTestCase
  setup do
    @nutrient = nutrients(:one)
  end

  test "visiting the index" do
    visit nutrients_url
    assert_selector "h1", text: "Nutrients"
  end

  test "creating a Nutrient" do
    visit nutrients_url
    click_on "New Nutrient"

    fill_in "Name", with: @nutrient.name
    fill_in "Unit name", with: @nutrient.unit_name
    click_on "Create Nutrient"

    assert_text "Nutrient was successfully created"
    click_on "Back"
  end

  test "updating a Nutrient" do
    visit nutrients_url
    click_on "Edit", match: :first

    fill_in "Name", with: @nutrient.name
    fill_in "Unit name", with: @nutrient.unit_name
    click_on "Update Nutrient"

    assert_text "Nutrient was successfully updated"
    click_on "Back"
  end

  test "destroying a Nutrient" do
    visit nutrients_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Nutrient was successfully destroyed"
  end
end
