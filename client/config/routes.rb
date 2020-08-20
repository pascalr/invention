Rails.application.routes.draw do
  resources :locations
  get 'location/clear/:id', to: 'locations#clear'
  resources :jars
  resources :recettes
  get 'image/show'
  get 'aliment/index'
  get 'heda/index'
  get 'heda/status'
  resources :heda, only: [:update, :show, :edit]
  post 'heda/run'
  post 'heda/run_recette'
  resources :jar_formats
  resources :shelves
  resources :ingredients
  resources :detected_codes
  root to: 'heda#index'
end
