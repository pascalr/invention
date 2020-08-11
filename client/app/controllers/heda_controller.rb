require 'net/https'
require 'open-uri'


class HedaController < ApplicationController
  def index
  end

  def status
    @output = 'some output'
    @pending = 'some pending'

    url = URI('http://192.168.0.19:8083/pollHeda')
    response = Net::HTTP.get(url)
    vals = JSON.parse(response)
    @output = vals.output
    @pending = vals.pending

    render partial: "status"
  end

  def run

    uri = URI('http://192.168.0.19:8083/run')
    req = Net::HTTP::Post.new(uri)
    req.set_form_data('cmd' => params[:cmd])
    
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
    
    render 'index'

    #url = URI.parse('http://127.0.0.1:8083/')
    #req = Net::HTTP::Post.new(url.path)
    #req.cmd = params[:cmd]
    #req.basic_auth url.user, url.password if url.user
    #con = Net::HTTP.new(url.host, url.port)
    #con.use_ssl = true
    #con.start {|http| http.request(req)}    
    #redirect_to(:back)
  end
end
