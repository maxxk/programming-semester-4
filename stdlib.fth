: if immediate
    ' 0branch , 
    here @ 
    0 ,
;

: then immediate 
    dup
    here @ swap -
    swap !
;

: else immediate
    ' branch ,
    here @ 
    0 ,
    swap 
    dup 
    here @ swap - 
    swap !
;

: test-if 1 = if 2 else 3 then ;


: [compile] immediate
	word
	find
	,
;

: begin immediate 
    here @
;

: until immediate
	' 0branch ,	
	here @ -	
	,
;

: again immediate
	' branch ,
	here @ -
	,
;

: while immediate
	' 0branch ,
	here @		
	0 , 
;

: repeat immediate
	' branch ,
	swap
	here @ - ,	
	dup
	here @ swap -	
	swap !	
;

: unless immediate
	' not ,		
	[compile] if
;

: test-loop begin 1 - dup dup while repeat ;

: do immediate 
    ' >r ,
    ' >r ,
    [compile] begin
;

: do-step 
r> r> r> 1 + over over < not swap >r swap >r swap >r ;

: do-cleanup r> r> r> drop drop >r ;

: loop immediate 
    ' do-step ,
    [compile] while
    [compile] repeat
    ' do-cleanup ,
;

: test-do 10 1 do i show 1 loop ;

: fib2 
   0 1 rot 0 do 
      over + swap loop 
    drop
;
 
: fib2-bench 2000 0 do i fib2 loop ;
