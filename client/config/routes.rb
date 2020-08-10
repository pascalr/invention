Rails.application.routes.draw do
  get 'heda/index'
  post 'heda/run'
  get 'configure/index'
  resources :jar_formats
  resources :shelves
  root to: 'heda#index'
end
