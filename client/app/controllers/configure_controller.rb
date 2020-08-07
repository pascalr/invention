class ConfigureController < ApplicationController
  def index
    @shelves = Shelf.all
    @jar_formats = JarFormat.all
  end
end
