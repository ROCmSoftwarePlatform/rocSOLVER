# ########################################################################
# HELPER FUNCTIONS
# ########################################################################

# ########################################################################
# target_link_libraries() override
# Wraps the normal cmake function to cope with hipcc/nvcc weirdness.
# ########################################################################
function( target_link_libraries target_name )
  # hipcc takes care of finding hip library dependencies internally; remove
  # explicit mentions of them so cmake doesn't complain on nvcc path
  # FIXME: This removes this target's hip libraries, even if this particular
  #        target is not built using a compiler that can find its dependencies
  #        internally (e.g. Fortran programs).
  if( CUDA_FOUND AND CMAKE_CXX_COMPILER MATCHES ".*/hipcc$|.*/nvcc$" )
    foreach( link_library ${ARGN} )
      if( (link_library MATCHES "^hip::") )
        message( DEBUG "Removing ${link_library} from ${target_name} library list" )
      else( )
        if( TARGET ${link_library} )
          list( APPEND new_list -Xlinker ${link_library} )
        else( )
          list( APPEND new_list ${link_library} )
        endif( )
      endif( )
    endforeach( )
    _target_link_libraries( ${target_name} ${new_list} )
  else( )
    _target_link_libraries( ${target_name} ${ARGN} )
  endif( )
endfunction( )

# ########################################################################
# A helper function to prefix a source list of files with a common path
# into a new list (non-destructive)
# ########################################################################
function( prepend_path prefix source_list_of_files return_list_of_files )
  foreach( file ${${source_list_of_files}} )
    if(IS_ABSOLUTE ${file} )
      list( APPEND new_list ${file} )
    else( )
      list( APPEND new_list ${prefix}/${file} )
    endif( )
  endforeach( )
  set( ${return_list_of_files} ${new_list} PARENT_SCOPE )
endfunction( )

# ########################################################################
# Search through the location properties to find one that is set
# ########################################################################
function( get_imported_target_location result_variable imported_target )
  set( properties_to_search
    IMPORTED_LOCATION
    IMPORTED_LOCATION_RELEASE
    IMPORTED_LOCATION_DEBUG)
  foreach( property ${properties_to_search} )
    get_target_property( location "${imported_target}" "${property}" )
    if( location )
      set( "${result_variable}" "${location}" PARENT_SCOPE )
      return()
    endif( )
  endforeach( )
  set( "${result_variable}" "${result_variable}-NOTFOUND" PARENT_SCOPE )
endfunction( )
