Rails.application.routes.draw do
  resources :colonnes
  resources :tags
  resources :recipe_categories
  resources :units
  resources :nutrients
  resources :fdcs
  resources :locations
  delete 'location/delete_all', to: 'locations#delete_all', as: 'delete_all_location'
  get 'location/clear/:id', to: 'locations#clear'
  resources :jars
  resources :recettes
  get 'image/show'
  get 'aliment/index'
  get 'heda/index'
  get 'heda/status'
  post 'heda/run'
  get 'heda/run'
  resources :heda, only: [:update, :show, :edit]
  post 'heda/run_recette'
  resources :jar_formats
  resources :shelves
  resources :ingredients
  resources :detected_codes
  root to: 'heda#index'
end
