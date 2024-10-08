require 'net/https'
require 'open-uri'


class HedaController < ApplicationController

  def index
    redirect_to heda_path(get_heda), params: request.query_parameters
  end

  def show
    expires_now()
    @heda = get_heda
    if params[:recette_id]
      @recette = Recette.find(params[:recette_id])
    end
    @heda = get_heda
  end

  def edit
    @shelves = Shelf.all
    @jar_formats = JarFormat.all
    @heda = get_heda
  end

  def update
    @heda ||= Heda.find(params[:id])
    #render 'edit', status: if @heda.update(heda_params) then :ok else :unprocessable_entity end
    if @heda.update(heda_params)
      execute('loadcfg')
      redirect_to action: 'edit'#, status: :ok
    else
      redirect_to action: 'edit'#, status: :unprocessable_entity
    end
  end

  def status

    url = URI(heda_uri('poll'))
    begin
      response = Net::HTTP.get(url)
    rescue => e
      @message = "An error of type #{e.class} happened, message is #{e.message}"
      render partial: "bad_connection"
      return
    end

    vals = JSON.parse(response)
    @output = vals["output"]
    @pending = vals["pending"]
    @position = vals["pos"]
    @tool_position = vals["toolPos"]
    @is_paused = vals["isPaused"]
    @status = vals["status"]
    @status_code = vals["status_code"]
    @action_required = vals["action_required"]

    logger.info "Vals: " + vals.to_s
    
    render partial: "status"
  end

  def run_recette
    recette = Recette.find(params[:recette_id])
    execute!(recette.instructions)

    redirect_to action: 'index', recette_id: params[:recette_id]
  end

  def run
    execute!(params[:cmd])
    
    redirect_to action: 'index'
  end

  private

  def get_heda
    heda = Heda.first
    if not heda
      heda = Heda.new
      if not heda.save
        throw "Empty Heda wont save!!!" + heda.errors.full_messages.to_s
      end
    end
    heda
  end

  def heda_uri(command)
    'http://127.0.0.1:8083/' + command
    #'http://192.168.0.19:8083/' + command
  end

  def execute!(cmd)
    uri = URI(heda_uri('run'))
    req = Net::HTTP::Post.new(uri)
    req.set_form_data('cmd' => cmd)
    
    res = Net::HTTP.start(uri.hostname, uri.port) do |http|
      http.request(req)
    end
    
    case res
    when Net::HTTPSuccess, Net::HTTPRedirection
      # OK
    else
      # error
      # res.value
    end
  end

  def execute(cmd)
    begin
      execute!(cmd)
      true
    rescue
      false
    end
  end

  def heda_params
    params.require(:heda).permit(:user_coord_offset_x, :user_coord_offset_y, :user_coord_offset_z, :shelf_id, :camera_radius, :gripper_radius, :camera_focal_point, :detect_height, :home_position_x, :home_position_y, :home_position_t, :grip_offset, :max_h, :max_v, :max_t, :closeup_distance, :max_x, :max_y, :max_z, :camera_width, :camera_height, :camera_calibration_height, :space_between_jaws, :grip_gap, :max_r)
  end

end
