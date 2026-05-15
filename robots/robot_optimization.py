"""
robot ecosystem operation code

This module creates a test ecosystem and runs it for a specified duration, 
demonstrating the use of the ecosystem factory and deliverable creation functions
It simulates the operation of delivery bots, including charging and delivering pizzas, while providing options for debugging and message display.
"""

from robots.ecosystem.ecosystem import energy_consumption
from robots.ecosystem.factory import ecofactory
from robots.ecosystem.ecosystem import distance

# Duration is set to two weeks for development and rapid testing. Set to 52 weeks for your final tests.

import matplotlib.pyplot as plt
plt.close('all')  # optional: cleans up leftovers from prior runs
plt.ion()         # interactive mode ON (non-blocking windows)

# Create and configure the ecosystem using the factory function. 
# Study the factory function code to understand how the ecosystem is being created and configured. Adjust the parameters as needed for your testing and development.                                                         # We are aiming to run for a year with minimum or no bot breakages

home = [40,20, 0]                                                               # Place to which bots will return when idle and from which they will start. This is also the location of the charger in this example, but it doesn't have to be. You can change this and the charger location to test the bots' ability to navigate around the ecosystem.

charge_threshold = { # variable charging thresholds for different bot kinds. Each was chosen after factoring in their weight class, carrying capacity, and charging capacity.
  'Robot': 0.25,
  'Droid': 0.20,
  'Drone': 0.30
}                            

def run_baseline(es): # function to run the default ecosystem without optimizations, to be used as a benchmark in the table against the optimized file
    es.display(show=0)
    es.messages_on = False
    es.duration = "52 week" # duration set to 52 weeks for final data collection.
    charger = es.chargers()[0]
    while es.active: # default decision making from the ecosystem_operation.py file
        for bot in es.bots():
          if bot.soc / bot.max_soc < 0.20 and bot.station is None:
              bot.charge(charger)
          if bot.activity == 'idle':
             for pizza in es.deliverables():
               if pizza.status == 'ready':
                  bot.deliver(pizza)
                  break
          if bot.target_destination:
            bot.move()
        es.update()
    return es


def find_nearest_charger(bot, es): #function to find nearest charger
  '''
  This function iterates through the list of chargers in the environment and calulates the distance from the bot
  to each charger. Keeping track of the nearest charger and its distance, it returns the nearest charger
  and it's distance so the bot can charge at the nearest one no matter where it is.
  '''
  nearest_charger = None
  min_distance = float('inf') #start with inf distance so any charger can be accepted
  
  for charger in es.chargers():
    if charger.status == 'vacant':
      d = distance(bot.coordinates, charger.coordinates) #in built distance function to calculate distance between bot and charger
      if d < min_distance:
        min_distance = d # updates minimum distance
        nearest_charger = charger # assigns nearest charger to bot
  
  return nearest_charger, min_distance

def select_optimized_pizza(bot, es): # function to select most optimal pizza
  '''
  This function iterates through the list of deliverables to find the closest pizza that is ready, and within the bot's
  carrying capacity. It then returns the optimal pizza for the bot to deliver.
  Simple for-loop optimization that iterates through every pizza and compares the distance to the bot.
  '''
  optimal_pizza = None # starts at none so it doesn't lock in a random pizza by accident
  min_distance = float ('inf') # set with inf distance so any pizza on the plot can be accepted.
  
  for pizza in es.deliverables():
    if pizza.status == 'ready' and pizza.weight <= bot.max_payload: # check if pizza is ready and within bot's carrying capacity
      closest_pizza_distance = distance(bot.coordinates, pizza.coordinates)
      if closest_pizza_distance < min_distance:
        min_distance = closest_pizza_distance # updates minimum distance to the closest pizza distance making it the optimal pizza
        optimal_pizza = pizza #assigning the optimal pizza to the bot
  return optimal_pizza
   
def run_optimized(es):
  es.display(show=0)
  es.messages_on = False
  es.duration = "52 week" #set to 52 weeks for final data collection and analysis
  while es.active:
    for bot in es.bots():

      #create_deliverables(es)                                                     # Use the create deliverables function to maintain a stock of ready pizzas

      nearest_charger, d = find_nearest_charger(bot, es) #find nearest charger for oppotunisitc charging, checks if it's within 3 units so it can charge before going to delivering the pizzas, minimizing damage.
      if d < 3 and bot.soc / bot.max_soc < (charge_threshold[bot.kind]+ 0.10) and bot.station is None: #if within 3 units and current charge is below threshold + 10%, robot goes for opportunistic charge
        bot.charge(nearest_charger) 

      #threshold charging
      elif bot.soc / bot.max_soc < charge_threshold[bot.kind] and bot.station is None:        # decision to charge when percent soc = 20%. This can be optimised and varied for each kind (see stretch objective)
        if nearest_charger:                                                                                                  # moves towards the charger                        
          bot.charge(nearest_charger) # uses bot.charge function to move and charge towards nearest charger

      if bot.activity == 'idle':                                                  
        pizza = select_optimized_pizza(bot, es)
        if pizza:
          bot.deliver(pizza) #if pizza is found, bot deliver function will tel the bot to deliver it
        if not bot.destination and bot.coordinates != home:
          bot.target_destination = home                                           # if we get here, we've gone through the list of pizzas and none was ready
      if bot.target_destination:bot.move()                                        # move whilst we have a destination. At the end of delivery, the bot status will be set to idle

    es.update()                                                              # update when all bots have been processed and moved
  return es

results = {} # dictionary to store the results of both baseline and optimized ecosystems to be compared in the table
results['baseline'] = run_baseline(ecofactory(robots = 3, droids = 3, drones = 3, chargers = [[1,15], [20, 3], [30, 25]], pizzas = 9, max_weight = 125))
results['optimized'] = run_optimized(ecofactory(robots = 3, droids = 3, drones = 3, chargers = [[1,15], [20, 3], [30, 25]], pizzas = 9, max_weight = 125))

print(f"\n{'Run':<12} {'Units':>8} {'Weight':>8} {'Distance':>10} {'Energy':>8} {'Damage':>8}") # labels for the table
print("---------------------------------------------------------") # seperator
for run, es in results.items():
    total_weight = sum(r['weight_delivered'] for r in es.registry(kind_class='Bot').values())
    total_units = sum(r['units_delivered'] for r in es.registry(kind_class='Bot').values())
    total_distance = sum(r['distance'] for r in es.registry(kind_class='Bot').values())
    total_energy = sum(r['energy'] for r in es.registry(kind_class='Bot').values())
    total_damage = sum(r['damage'] for r in es.registry(kind_class='Bot').values())
    print(f"{run:<12} {total_units:>8} {total_weight:>8} {total_distance:>10.1f} {total_energy:>8.1f} {total_damage:>8}") # formatted table using f strings using values from the result dictionary