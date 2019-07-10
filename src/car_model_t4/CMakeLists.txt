cmake_minimum_required(VERSION 2.8.3)

project(car_model_t4)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  geometry_msgs
  tf
)

catkin_package()

find_package(roslaunch)

foreach(dir config launch meshes urdf)
	install(DIRECTORY ${dir}/
		DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${dir})
endforeach(dir)

include_directories(include ${catkin_INCLUDE_DIRS} ${PCL_INCLUDE_DIRS})

# catkin_add_nosetests(test)
add_executable(rpy_angles src/rpy_angles.cpp)
#add_dependencies(rpy_angles ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(rpy_angles ${catkin_LIBRARIES})