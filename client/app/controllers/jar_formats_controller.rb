class JarFormatsController < ApplicationController
  def index
    @jar_formats = JarFormat.all
  end

  def new
    @jar_format = JarFormat.new
  end

  def edit
    @jar_format = JarFormat.find(params[:id])
  end

  def create
    @jar_format = JarFormat.new(jar_format_params)

    if @jar_format.save
      redirect_to jar_formats_path
    else
      render 'new'
    end
  end

  def update
    @jar_format = JarFormat.find(params[:id])

    if @jar_format.update(jar_format_params)
      redirect_to jar_formats_path
    else
      render 'edit'
    end
  end

  def destroy
    @jar_format = JarFormat.find(params[:id])
    @jar_format.destroy

    redirect_to jar_formats_path
  end

  private
    def jar_format_params
      params.require(:jar_format).permit(:name, :empty_weight, :height, :diameter, :lid_diameter, :lid_weight)
    end
end
