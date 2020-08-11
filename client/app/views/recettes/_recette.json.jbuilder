json.extract! recette, :id, :name, :instructions, :rating, :created_at, :updated_at
json.url recette_url(recette, format: :json)
