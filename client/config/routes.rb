Rails.application.routes.draw do
  get 'aliment/index'
  get 'heda/index'
  post 'heda/run'
  get 'configure/index'
  resources :jar_formats
  resources :shelves
  resources :ingredients
  root to: 'heda#index'
end
