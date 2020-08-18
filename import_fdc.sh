#!/bin/bash
# rails d model Fdc
rails g scaffold Fdc data_type:string description:string food_category_id:int publication_date:string

# rails d model Nutrient
rails g scaffold Nutrient name:string unit_name:string

# rails d model FdcNutrient
rails g model FdcNutrient fdc:references nutrient:references amount:float median:float

# rails g task fdc import
rake fdc:import

