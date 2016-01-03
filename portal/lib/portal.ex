defmodule Portal do
  use Application

  def start(_type, _args) do
    import Supervisor.Spec, warn: false

    # Doors are spawned as seperated workers at time of 'shoot'
    # If a Door process is killed it will lose all data and
    # a new process will be spawned
    children = [
      worker(Portal.Door, [])
    ]

    opts = [strategy: :simple_one_for_one, name: Portal.Supervisor]
    Supervisor.start_link(children, opts)
  end

  @doc """
  Shoots a new door with a given `color`
  """
  def shoot(color) do
    Supervisor.start_child(Portal.Supervisor, [color])
  end

  @doc """
  Representation of a pair of doors that make up a portal
  """
  defstruct [:left, :right]

  @doc """
  Starts transfering `data` from `left` to `right`
  """
  def transfer(left, right, data) do
    # Ensure they're joined
    portal = join(left, right)

    # Add all data to the portal on the left
    for item <- data do
      Portal.Door.push(left, item)
    end

    portal
  end

  @doc """
  Joins two existing doors in a portal and returns structure to reference it
  """
  def join(left, right) do
    %Portal{left: left, right: right}
  end

  @doc """
  Pushes data to the right in a given `portal`
  """
  def push_right(portal) do
    push(portal.left, portal.right)

    # Return the refernece back to the portal
    portal
  end

  @doc """
  Pushes data to the left in a given `portal`
  """
  def push_left(portal) do
    push(portal.right, portal.left)

    # Return the refernece back to the portal
    portal
  end

  @doc """
  Pushes data `from` one door `to` another
  """
  def push(from, to) do
    # See if we can pop data `from`.
    #   If so, push the poped data `to`
    #   Otherwise, do nothing

    # Kind of strange to see :error -> :ok, but the Door returns an error if there's nothing to pop.
    # Our API is fine with that.
    case Portal.Door.pop(from) do
      :error    -> :ok
      {:ok, h}  -> Portal.Door.push(to, h)
    end
  end
end

# Debugging tool to get better feedback on the portal in iex
defimpl Inspect, for: Portal do
  def inspect(%Portal{left: left, right: right}, _) do
    left_door   = inspect(left)
    right_door  = inspect(right)

    left_data   = inspect(Enum.reverse(Portal.Door.get(left)))
    right_data  = inspect(Portal.Door.get(right))

    max = max(String.length(left_door), String.length(left_data))

    """
    #Portal<
      #{String.rjust(left_door, max)} <=> #{right_door}
      #{String.rjust(left_data, max)} <=> #{right_data}
    >
    """
  end
end