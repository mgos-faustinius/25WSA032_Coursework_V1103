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
# Study the factory function code to understand how the ecosystem is being created 
# and configured. Adjust the parameters as needed for your testing and development.  
es = ecofactory(robots = 3, droids = 3, drones = 3, chargers = [[1,15], [20, 3], [30, 25]], pizzas = 9, max_weight = 125) # increased max weight to 125 to account for heaviest possible pizza robot can take



charger = es.chargers()[0]
es.display(show = 1, pause = 10)                                                # show = 0 will turn off the display and speed up the run. Set to 1 for development and debugging, set to 0 for final runs. Note that when show = 0, you will not see the ecosystem or any messages, so it is wise to turn on messages (es.messages_on = True) when show = 0 for development and debugging. 
es.debug = False                                                                # this will directly display damage and warning messages. Note show needs to be zero  (show = 0)
es.messages_on = False                                                          # over 52 weeks it is wise to turn messages off as there are too many. But when researching turn on for shorter runs
es.duration = "2 week"                                                          # We are aiming to run for a year with minimum or no bot breakages

home = [40,20, 0]                                                               # Place to which bots will return when idle and from which they will start. This is also the location of the charger in this example, but it doesn't have to be. You can change this and the charger location to test the bots' ability to navigate around the ecosystem.

charge_threshold = { # variable charging thresholds for different bot kinds. 
  'Robot': 0.25,
  'Droid': 0.20,
  'Drone': 0.30
}                            

def run_baseline(es):
    es.display(show=0)
    es.messages_on = False
    es.duration = "52 week"
    charger = es.chargers()[0]
    while es.active:
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
  optimal_pizza = None
  min_distance = float ('inf')
  
  for pizza in es.deliverables():
    if pizza.status == 'ready' and pizza.weight <= bot.max_payload: # check if pizza is ready and within bot's carrying capacity
      closest_pizza_distance = distance(bot.coordinates, pizza.coordinates)
      if closest_pizza_distance < min_distance:
        min_distance = closest_pizza_distance 
        optimal_pizza = pizza 
  return optimal_pizza
   
def run_optimized(es):
  es.display(show=0)
  es.messages_on = False
  es.duration = "52 week"
  while es.active:
    for bot in es.bots():

      #create_deliverables(es)                                                     # Use the create deliverables function to maintain a stock of ready pizzas

      nearest_charger, d = find_nearest_charger(bot, es) #find nearest charger for oppotunisitc charging
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

results = {}
results['baseline'] = run_baseline(ecofactory(robots = 3, droids = 3, drones = 3, chargers = [[1,15], [20, 3], [30, 25]], pizzas = 9, max_weight = 125))
results['optimized'] = run_optimized(ecofactory(robots = 3, droids = 3, drones = 3, chargers = [[1,15], [20, 3], [30, 25]], pizzas = 9, max_weight = 125))

print(f"\n{'Run':<12} {'Units':>8} {'Weight':>8} {'Distance':>10} {'Energy':>8} {'Damage':>8}")
print("-" * 56)
for run, es in results.items():
    total_weight = sum(r['weight_delivered'] for r in es.registry(kind_class='Bot').values())
    total_units = sum(r['units_delivered'] for r in es.registry(kind_class='Bot').values())
    total_distance = sum(r['distance'] for r in es.registry(kind_class='Bot').values())
    total_energy = sum(r['energy'] for r in es.registry(kind_class='Bot').values())
    total_damage = sum(r['damage'] for r in es.registry(kind_class='Bot').values())
    print(f"{run:<12} {total_units:>8} {total_weight:>8} {total_distance:>10.1f} {total_energy:>8.1f} {total_damage:>8}")