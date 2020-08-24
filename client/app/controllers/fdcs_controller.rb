class FdcsController < ApplicationController
  before_action :set_fdc, only: [:show, :edit, :update, :destroy]

  # GET /fdcs
  # GET /fdcs.json
  def index
    @fdcs = Fdc.all
  end

  # GET /fdcs/1
  # GET /fdcs/1.json
  def show
  end

  # GET /fdcs/new
  def new
    @fdc = Fdc.new
  end

  # GET /fdcs/1/edit
  def edit
  end

  # POST /fdcs
  # POST /fdcs.json
  def create
    @fdc = Fdc.new(fdc_params)

    respond_to do |format|
      if @fdc.save
        format.html { redirect_to @fdc, notice: 'Fdc was successfully created.' }
        format.json { render :show, status: :created, location: @fdc }
      else
        format.html { render :new }
        format.json { render json: @fdc.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /fdcs/1
  # PATCH/PUT /fdcs/1.json
  def update
    respond_to do |format|
      if @fdc.update(fdc_params)
        format.html { redirect_to @fdc, notice: 'Fdc was successfully updated.' }
        format.json { render :show, status: :ok, location: @fdc }
      else
        format.html { render :edit }
        format.json { render json: @fdc.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /fdcs/1
  # DELETE /fdcs/1.json
  def destroy
    @fdc.destroy
    respond_to do |format|
      format.html { redirect_to fdcs_url, notice: 'Fdc was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_fdc
      @fdc = Fdc.find(params[:id])
    end

    # Only allow a list of trusted parameters through.
    def fdc_params
      params.require(:fdc).permit(:data_type, :description, :food_category_id, :publication_date)
    end
end
