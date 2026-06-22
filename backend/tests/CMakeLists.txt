cmake_minimum_required(VERSION 3.5)
project(backend_test CXX)

aux_source_directory(../algorithm/planner TEST_ALGORITHM_PLANNER_SRC)
aux_source_directory(../algorithm/adjuster TEST_ALGORITHM_ADJUSTER_SRC)
aux_source_directory(../algorithm/explainer TEST_ALGORITHM_EXPLAINER_SRC)
aux_source_directory(../controllers TEST_CONTROLLER_SRC)
aux_source_directory(../services TEST_SERVICE_SRC)
aux_source_directory(../repositories TEST_REPOSITORY_SRC)
aux_source_directory(../utils TEST_UTIL_SRC)

add_executable(${PROJECT_NAME}
               test_main.cc
               AuthSessionServiceTest.cc
               KnowledgeGraphServiceTest.cc
               RoleDashboardServiceTest.cc
               PathPlannerTest.cc
               PathPlanningServiceTest.cc
               PathAdjusterTest.cc
               PathExplanationTest.cc
               LearnerProfileFeedbackServiceTest.cc
                TeacherStudentServiceTest.cc
                TeacherCourseEditServiceTest.cc
                TeacherQuestionBankServiceTest.cc
                TestSupport.cc)
target_sources(${PROJECT_NAME}
               PRIVATE
               ${TEST_CONTROLLER_SRC}
               ${TEST_ALGORITHM_PLANNER_SRC}
               ${TEST_ALGORITHM_ADJUSTER_SRC}
               ${TEST_ALGORITHM_EXPLAINER_SRC}
               ${TEST_SERVICE_SRC}
               ${TEST_REPOSITORY_SRC}
               ${TEST_UTIL_SRC})
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon 
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon sqlite3 OpenSSL::SSL OpenSSL::Crypto)

ParseAndAddDrogonTests(${PROJECT_NAME})
