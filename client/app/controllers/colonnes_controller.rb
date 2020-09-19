class ColonnesController < ApplicationController
  before_action :set_colonne, only: [:show, :edit, :update, :destroy]

  # GET /colonnes
  # GET /colonnes.json
  def index
    @colonnes = Colonne.all
  end

  # GET /colonnes/1
  # GET /colonnes/1.json
  def show
  end

  # GET /colonnes/new
  def new
    @colonne = Colonne.new
  end

  # GET /colonnes/1/edit
  def edit
  end

  # POST /colonnes
  # POST /colonnes.json
  def create
    @colonne = Colonne.new(colonne_params)

    respond_to do |format|
      if @colonne.save
        format.html { redirect_to @colonne, notice: 'Colonne was successfully created.' }
        format.json { render :show, status: :created, location: @colonne }
      else
        format.html { render :new }
        format.json { render json: @colonne.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /colonnes/1
  # PATCH/PUT /colonnes/1.json
  def update
    respond_to do |format|
      if @colonne.update(colonne_params)
        format.html { redirect_to @colonne, notice: 'Colonne was successfully updated.' }
        format.json { render :show, status: :ok, location: @colonne }
      else
        format.html { render :edit }
        format.json { render json: @colonne.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /colonnes/1
  # DELETE /colonnes/1.json
  def destroy
    @colonne.destroy
    respond_to do |format|
      format.html { redirect_to colonnes_url, notice: 'Colonne was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_colonne
      @colonne = Colonne.find(params[:id])
    end

    # Only allow a list of trusted parameters through.
    def colonne_params
      params.require(:colonne).permit(:name, :shelf_id, :width, :center_x)
    end
end
