defmodule PortalTest do
  use ExUnit.Case, async: true

  test "Construction" do
    Portal.shoot(:manual_left)
    Portal.shoot(:manual_right)

    portal = Portal.join(:manual_left, :manual_right)

    # These won't move data but will ensure we read from each side
    Portal.push_right(portal)
    Portal.push_left(portal)
  end

  test "Initializing with data" do
    Portal.shoot(:orange)
    Portal.shoot(:blue)

    Portal.transfer(:orange, :blue, [1,2,3,4])
    assert [4,3,2,1] == Portal.Door.get(:orange)
    assert [] == Portal.Door.get(:blue)
  end

  test "Shifting" do
    Portal.shoot(:green)
    Portal.shoot(:black)

    assert [] == Portal.Door.get(:green)
    assert [] == Portal.Door.get(:black)

    portal = Portal.transfer(:green, :black, [3,2,1])
    
    Portal.push_right(portal)
    assert [2,3] == Portal.Door.get(:green)
    assert [1]   == Portal.Door.get(:black)

    Portal.push_left(portal)
    assert [1,2,3] == Portal.Door.get(:green)
    assert []      == Portal.Door.get(:black)

    Portal.push_right(portal)
    Portal.push_right(portal)
    Portal.push_right(portal)
    assert []      == Portal.Door.get(:green)
    assert [3,2,1] == Portal.Door.get(:black)
  end
end