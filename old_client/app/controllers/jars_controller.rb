class JarsController < ApplicationController
  before_action :set_jar, only: [:show, :edit, :update, :destroy]

  # GET /jars
  # GET /jars.json
  def index
    @jars = Jar.order(:jar_id).all
  end

  # GET /jars/1
  # GET /jars/1.json
  def show
  end

  # GET /jars/new
  def new
    @jar = Jar.new
  end

  # GET /jars/1/edit
  def edit
  end

  # POST /jars
  # POST /jars.json
  def create
    @jar = Jar.new(jar_params)

    # in the odd case that the jar id was misread, it might already exists, discard it.
    old_jar = Jar.find_by(jar_id: @jar.jar_id)

    if params[:detected_id]
      code = DetectedCode.find(params[:detected_id])
      if @jar.save
        old_jar.destroy if old_jar
        code.jar_id = "%03i" % @jar.jar_id
        code.save
        redirect_to controller: 'heda', action: 'run', cmd: 'done'
      else
        render :new
      end
    else
      respond_to do |format|
        if @jar.save
          format.html { redirect_to @jar, notice: 'Jar was successfully created.' }
          format.json { render :show, status: :created, location: @jar }
        else
          format.html { render :new }
          format.json { render json: @jar.errors, status: :unprocessable_entity }
        end
      end
    end

  end

  # PATCH/PUT /jars/1
  # PATCH/PUT /jars/1.json
  def update
    respond_to do |format|
      if @jar.update(jar_params)
        format.html { redirect_to @jar, notice: 'Jar was successfully updated.' }
        format.json { render :show, status: :ok, location: @jar }
      else
        format.html { render :edit }
        format.json { render json: @jar.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /jars/1
  # DELETE /jars/1.json
  def destroy
    @jar.destroy
    respond_to do |format|
      format.html { redirect_to jars_url, notice: 'Jar was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_jar
      @jar = Jar.find(params[:id])
    end

    # Only allow a list of trusted parameters through.
    def jar_params
      params.require(:jar).permit(:jar_format_id, :ingredient_id, :location_id, :jar_id)
    end
end
