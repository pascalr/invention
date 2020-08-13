class DetectedCodesController < ApplicationController
  before_action :set_detected_code, only: [:show, :edit, :update, :destroy]

  # GET /detected_codes
  # GET /detected_codes.json
  def index
    @detected_codes = DetectedCode.all
  end

  # GET /detected_codes/1
  # GET /detected_codes/1.json
  def show
  end

  # GET /detected_codes/new
  def new
    @detected_code = DetectedCode.new
  end

  # GET /detected_codes/1/edit
  def edit
  end

  # POST /detected_codes
  # POST /detected_codes.json
  def create
    @detected_code = DetectedCode.new(detected_code_params)

    respond_to do |format|
      if @detected_code.save
        format.html { redirect_to @detected_code, notice: 'Detected code was successfully created.' }
        format.json { render :show, status: :created, location: @detected_code }
      else
        format.html { render :new }
        format.json { render json: @detected_code.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /detected_codes/1
  # PATCH/PUT /detected_codes/1.json
  def update
    respond_to do |format|
      if @detected_code.update(detected_code_params)
        format.html { redirect_to @detected_code, notice: 'Detected code was successfully updated.' }
        format.json { render :show, status: :ok, location: @detected_code }
      else
        format.html { render :edit }
        format.json { render json: @detected_code.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /detected_codes/1
  # DELETE /detected_codes/1.json
  def destroy
    @detected_code.destroy
    respond_to do |format|
      format.html { redirect_to detected_codes_url, notice: 'Detected code was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_detected_code
      @detected_code = DetectedCode.find(params[:id])
    end

    # Only allow a list of trusted parameters through.
    def detected_code_params
      params.require(:detected_code).permit(:x, :y, :t, :centerX, :centerY, :scale, :img)
    end
end
