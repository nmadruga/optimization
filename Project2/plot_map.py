import folium
import pandas as pd

# Function to plot the optimal path on a map
def plot_optimal_path(locations):
    # Create a map centered around Spain
    spain_map = folium.Map(location=[40.4168, -3.7038], zoom_start=6)

    # Draw a polyline without markers
    folium.PolyLine(locations, color="blue", weight=2.5, opacity=1).add_to(spain_map)
    
    # Save the map as an HTML file
    spain_map.save("optimal_path_map.html")

# Read latitude and longitude values from a CSV file
df = pd.read_csv('data/spain_result.csv')  # Replace 'your_file.csv' with your actual file name

# Extract latitude and longitude columns from the DataFrame
latitudes = df['Latitude'].tolist()
longitudes = df['Longitude'].tolist()

# Create a list of coordinates for the optimal path
optimal_path_coordinates = list(zip(latitudes, longitudes))

# Plot the optimal path and polygon on the map
plot_optimal_path(optimal_path_coordinates)