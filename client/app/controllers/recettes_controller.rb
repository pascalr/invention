class RecettesController < ApplicationController
  before_action :set_recette, only: [:show, :edit, :update, :destroy]

  # GET /recettes
  # GET /recettes.json
  def index
    @recettes = Recette.all
  end

  # GET /recettes/1
  # GET /recettes/1.json
  def show
  end

  # GET /recettes/new
  def new
    @recette = Recette.new
  end

  # GET /recettes/1/edit
  def edit
  end

  # POST /recettes
  # POST /recettes.json
  def create
    @recette = Recette.new(recette_params)

    respond_to do |format|
      if @recette.save
        format.html { redirect_to @recette, notice: 'Recette was successfully created.' }
        format.json { render :show, status: :created, location: @recette }
      else
        format.html { render :new }
        format.json { render json: @recette.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /recettes/1
  # PATCH/PUT /recettes/1.json
  def update
    respond_to do |format|
      if @recette.update(recette_params)
        format.html { redirect_to @recette, notice: 'Recette was successfully updated.' }
        format.json { render :show, status: :ok, location: @recette }
      else
        format.html { render :edit }
        format.json { render json: @recette.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /recettes/1
  # DELETE /recettes/1.json
  def destroy
    @recette.destroy
    respond_to do |format|
      format.html { redirect_to recettes_url, notice: 'Recette was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_recette
      @recette = Recette.find(params[:id])
    end

    # Only allow a list of trusted parameters through.
    def recette_params
      params.require(:recette).permit(:name, :instructions, :rating, :recipe_category_id)
    end
end
