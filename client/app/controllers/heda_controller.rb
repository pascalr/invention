require 'net/https'
require 'open-uri'


class HedaController < ApplicationController

  def index
    if params[:recette_id]
      @recette = Recette.find(params[:recette_id])
    end
  end

  def status

    url = URI('http://192.168.0.19:8083/poll')
    response = Net::HTTP.get(url)
    vals = JSON.parse(response)
    @output = vals["output"]
    @pending = vals["pending"]
    @position = vals["pos"]

    render partial: "status"
  end

  def run_recette
    recette = Recette.find(params[:recette_id])
    execute(recette.instructions)

    redirect_to action: 'index', recette_id: params[:recette_id]
  end

  def run
    execute(params[:cmd])
    
    redirect_to action: 'index'
  end

  private

  def execute(cmd)
    uri = URI('http://192.168.0.19:8083/run')
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

end
