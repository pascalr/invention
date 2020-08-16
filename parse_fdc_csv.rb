require 'csv'

#https://www.justinweiss.com/articles/writing-a-one-time-script-in-rails/
rails g task fdc import


rails g model FdcFood fdc_id:int data_type:string description:string food_category_id:int publication_date:string
FdcFood.delete_all
#"fdc_id","data_type","description","food_category_id","publication_date"
filename = "data/ingredients/food.csv"
table = CSV.parse(File.read(filename), headers: true)
table.each { |row|
  food = FdcFood.new
  food.fdc_id = row.fdc_id
  food.data_type = row.data_type
  food.description = row.description
  food.food_category_id = row.food_category_id
  food.publication_date = row.publication_date
}
# puts table.headers

