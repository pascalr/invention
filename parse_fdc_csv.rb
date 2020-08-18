require 'csv'

rails g task fdc import

# FdcFood.delete_all
#"fdc_id","data_type","description","food_category_id","publication_date"
filename = "data/ingredients/food.csv"
table = CSV.parse(File.read(filename), headers: true)
table.each { |row|
  item = Fdc.new
  item.id = row.fdc_id
  item.data_type = row.data_type
  item.description = row.description
  item.food_category_id = row.food_category_id
  item.publication_date = row.publication_date
  item.save!
}

# id name unit_name nutrien_nbr
filename = "data/ingredients/nutrient.csv"
table = CSV.parse(File.read(filename), headers: true)
table.each { |row|
  item = Nutrient.new
  item.id = row["id"]
  item.name = row["name"]
  item.unit_name = row["unit_name"]
  item.save!
}

#id	fdc_id	nutrient_id	amount	data_points	derivation_id	min	max	median	footnote	min_year_acquired
filename = "data/ingredients/food_nutrient.csv"
table = CSV.parse(File.read(filename), headers: true)
table.each { |row|
  item = FdcNutrient.new
  item.id = row["id"]
  item.fdc_id = row["fdc_id"]
  item.nutrient_id = row["nutrient_id"]
  item.amount = row["amount"]
  item.median = row["median"]
  item.save!
}
