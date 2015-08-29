defmodule DoorTest do
  use ExUnit.Case, async: true

  test "Empty door operations" do
    Portal.Door.start_link(:door_1)

    assert []     == Portal.Door.get(:door_1)
    assert :error == Portal.Door.pop(:door_1)
  end

  test "Door with data" do
    Portal.Door.start_link(:door_2)

    Portal.Door.push(:door_2, 1)
    assert [1]      == Portal.Door.get(:door_2)
    assert {:ok, 1} == Portal.Door.pop(:door_2)
    assert []       == Portal.Door.get(:door_2)

    Portal.Door.push(:door_2, 3)
    Portal.Door.push(:door_2, 2)
    Portal.Door.push(:door_2, 1)

    assert [1,2,3]  == Portal.Door.get(:door_2)
    assert {:ok, 1} == Portal.Door.pop(:door_2)
    assert {:ok, 2} == Portal.Door.pop(:door_2)
    assert {:ok, 3} == Portal.Door.pop(:door_2)
    assert :error   == Portal.Door.pop(:door_2)
  end
end