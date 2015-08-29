Portal
======

Example app built from (How I Start Tutorial)[https://howistart.org/posts/elixir/1]

## Running the Application ##

Elixir: (Instructions)[http://elixir-lang.org/install.html]
Test suite: `mix test`
Interactive shell: `iex -S mix`

## Sample Usage ##

````elixir
Portal.shoot(:blue)
Portal.shoot(:orange)

portal = Portal.transfer(:blue, :orange, [1,2,3])
# #Portal<
#      :blue <=> :orange
#  [1, 2, 3] <=> []
# >

Portal.push_right(portal)
# #Portal<
#   :blue <=> :orange
#   [1, 2] <=> [3]
# >

Portal.push_left(portal)
# #Portal<
#       :blue <=> :orange
#   [1, 2, 3] <=> []
# >
````

## On multiple machines/VMs ##

An example of the power of Elixir/Erlang. Portals can be shot on different machiens/VM instances and communicated across.

### Process 1 ###
    C:\Users\jeffrey.VERTPEARL\portal>iex --sname room1 --cookie secret -S mix
    Interactive Elixir (1.0.5) - press Ctrl+C to exit (type h() ENTER for help)
    iex(room1@freedgoscore)1> Portal.shoot(:blue)
    {:ok, #PID<0.91.0>}
    iex(room1@freedgoscore)2> orange = {:orange, :"room2@freedgoscore"}
    {:orange, :room2@freedgoscore}
    iex(room1@freedgoscore)3> blue = {:blue, :"room1@freedgoscore"}
    {:blue, :room1@freedgoscore}

### Process 2 ###
    C:\Users\jeffrey.VERTPEARL\portal>iex --sname room2 --cookie secret -S mix
    Interactive Elixir (1.0.5) - press Ctrl+C to exit (type h() ENTER for help)
    iex(room2@freedgoscore)1> Portal.shoot(:orange)
    {:ok, #PID<0.91.0>}
    iex(room2@freedgoscore)2> orange = {:orange, :"room1@freedgoscore"}
    {:orange, :room1@freedgoscore}
    iex(room2@freedgoscore)3> blue = {:blue, :"room1@freedgoscore"}
    {:blue, :room1@freedgoscore}
    iex(room2@freedgoscore)4> p = Portal.join(orange, blue)
    #Portal<
      {:orange, :room2@freedgoscore} <=> {:blue, :room1@freedgoscore}
                                  [] <=> []
    >

### Process 1 ###
    iex(room1@freedgoscore)4> Portal.transfer(blue, orange, [1,2,3])
    #Portal<
      {:blue, :room1@freedgoscore} <=> {:orange, :room2@freedgoscore}
                         [1, 2, 3] <=> []
    >

### Process 2 ###
    iex(room2@freedgoscore)6> p
    #Portal<
      {:orange, :room2@freedgoscore} <=> {:blue, :room1@freedgoscore}
                                  [] <=> [3, 2, 1]
    >