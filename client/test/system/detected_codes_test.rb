require "application_system_test_case"

class DetectedCodesTest < ApplicationSystemTestCase
  setup do
    @detected_code = detected_codes(:one)
  end

  test "visiting the index" do
    visit detected_codes_url
    assert_selector "h1", text: "Detected Codes"
  end

  test "creating a Detected code" do
    visit detected_codes_url
    click_on "New Detected Code"

    fill_in "Centerx", with: @detected_code.centerX
    fill_in "Centery", with: @detected_code.centerY
    fill_in "Img", with: @detected_code.img
    fill_in "Scale", with: @detected_code.scale
    fill_in "T", with: @detected_code.t
    fill_in "X", with: @detected_code.x
    fill_in "Y", with: @detected_code.y
    click_on "Create Detected code"

    assert_text "Detected code was successfully created"
    click_on "Back"
  end

  test "updating a Detected code" do
    visit detected_codes_url
    click_on "Edit", match: :first

    fill_in "Centerx", with: @detected_code.centerX
    fill_in "Centery", with: @detected_code.centerY
    fill_in "Img", with: @detected_code.img
    fill_in "Scale", with: @detected_code.scale
    fill_in "T", with: @detected_code.t
    fill_in "X", with: @detected_code.x
    fill_in "Y", with: @detected_code.y
    click_on "Update Detected code"

    assert_text "Detected code was successfully updated"
    click_on "Back"
  end

  test "destroying a Detected code" do
    visit detected_codes_url
    page.accept_confirm do
      click_on "Destroy", match: :first
    end

    assert_text "Detected code was successfully destroyed"
  end
end
