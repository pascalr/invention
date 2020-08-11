Rails.application.routes.draw do
  resources :recettes
  get 'aliment/index'
  get 'heda/index'
  post 'heda/run'
  get 'configure/index'
  resources :jar_formats
  resources :shelves
  resources :ingredients
  root to: 'heda#index'
end
