class AddCameraCalibrationHeightToHedas < ActiveRecord::Migration[6.0]
  def change
    add_column :hedas, :camera_calibration_height, :float
  end
end
