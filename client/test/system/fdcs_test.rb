require "application_system_test_case"

class FdcsTest < ApplicationSystemTestCase
  setup do
    @fdc = fdcs(:one)
  end

  test "visiting the index" do
    visit fdcs_url
    assert_selector "h1", text: "Fdcs"
  end

  test "creating a Fdc" do
    visit fdcs_url
    click_on "New Fdc"

    fill_in "Data type", with: @fdc.data_type
    fill_in "Description", with: @fdc.description
    fill_in "Food category", with: @fdc.food_category_id
    fill_in "Publication date", with: @fdc.publication_date
    click_on "Create Fdc"

    assert_text "Fdc was successfully created"
    click_on "Back"
  end

  test "updating a Fdc" do
    visit fdcs_url
    click_on "Edit", match: :first

    fill_in "Data type", with: @fdc.data_type
    fill_in "Description", with: @fdc.description
    fill_in "Food category", with: @fdc.food_category_id
    fill_in "Publication date", with: @fdc.publication_date
    click_on "Update Fdc"

    assert_text "Fdc was successfully updated"
    click_on "Back"
  end

  test "destroying a Fdc" do
    visit fdcs_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Fdc was successfully destroyed"
  end
end
