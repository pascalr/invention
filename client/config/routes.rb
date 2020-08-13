Rails.application.routes.draw do
  resources :jars
  resources :recettes
  get 'aliment/index'
  get 'heda/index'
  get 'heda/status'
  post 'heda/run'
  post 'heda/run_recette'
  get 'configure/index'
  resources :jar_formats
  resources :shelves
  resources :ingredients
  resources :detected_codes
  root to: 'heda#index'
end
