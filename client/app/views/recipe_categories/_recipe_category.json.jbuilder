json.extract! recipe_category, :id, :name, :recipe_category_id, :created_at, :updated_at
json.url recipe_category_url(recipe_category, format: :json)
