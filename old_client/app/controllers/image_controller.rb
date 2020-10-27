class ImageController < ApplicationController
  def show
    name = File.basename(params[:name]) # sanitize name to avoid malicious users to read any file
    send_file "#{Rails.root}/storage/detected_codes/#{name}", type: 'image/jpg', disposition: 'inline'
  end
end
