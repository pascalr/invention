
namespace :fdc do
  desc "TODO"
  task import: :environment do
    require 'csv'

    # rails g scaffold Fdc data_type:string description:string food_category_id:int publication_date:string
    #Fdc.delete_all
    #filename = "../data/ingredients/food.csv"
    #table = CSV.parse(File.read(filename), headers: true)
    #table.each { |row|
    #  item = Fdc.new
    #  item.id = row["fdc_id"]
    #  item.data_type = row["data_type"]
    #  item.description = row["description"]
    #  item.food_category_id = row["food_category_id"]
    #  item.publication_date = row["publication_date"]
    #  item.save!
    #}

    ## rails g scaffold Nutrient name:string unit_name:string
    #Nutrient.delete_all
    #filename = "../data/ingredients/nutrient.csv"
    #table = CSV.parse(File.read(filename), headers: true)
    #table.each { |row|
    #  item = Nutrient.new
    #  item.id = row["id"]
    #  item.name = row["name"]
    #  item.unit_name = row["unit_name"]
    #  item.save!
    #}
   
    FdcNutrient.delete_all 
    # rails g model FdcNutrient fdc:references nutrient:references amount:float median:float
    # split food_nutrient.csv food_nutrient_ -n l/26
    filenames = ('a'..'z').map{|c| "../data/ingredients/food_nutrient/food_nutrient_a" + c}
    filenames.each do |filename|
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
    end
  end

end

