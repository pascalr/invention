Rails.application.routes.draw do
  get 'manual/index'
  get 'configure/index'
  resources :jar_formats
  resources :shelves
  root to: 'manual#index'
end
