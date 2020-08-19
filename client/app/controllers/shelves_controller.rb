class ShelvesController < ApplicationController
  def index
    @shelves = Shelf.all
  end

  def new
    @shelf = Shelf.new
  end

  def edit
    @shelf = Shelf.find(params[:id])
  end

  def create
    @shelf = Shelf.new(shelf_params)

    if @shelf.save
      redirect_to shelves_path
    else
      render 'new'
    end
  end

  def update
    @shelf = Shelf.find(params[:id])

    if @shelf.update(shelf_params)
      redirect_to shelves_path
    else
      render 'edit'
    end
  end

  def destroy
    @shelf = Shelf.find(params[:id])
    @shelf.destroy

    redirect_to shelves_path
  end

  private
    def shelf_params
      params.require(:shelf).permit(:height, :width, :depth, :offset_x, :offset_z, :moving_height)
    end
end
