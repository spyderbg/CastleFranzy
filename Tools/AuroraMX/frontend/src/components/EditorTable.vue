<script setup lang="ts">
const p = defineProps<{
  columns: string[];
  rows: (string | number)[][];
  selected: number;
  label: string;
}>();
const emit = defineEmits<{
  select: [index: number];
  dblclick: [index: number];
}>();
function navigate(event: KeyboardEvent, index: number) {
  if (event.key === "ArrowDown") {
    event.preventDefault();
    emit("select", Math.min(p.rows.length - 1, index + 1));
  }
  if (event.key === "ArrowUp") {
    event.preventDefault();
    emit("select", Math.max(0, index - 1));
  }
}
</script>
<template>
  <div class="table-scroll">
    <table :aria-label="p.label">
      <thead>
        <tr>
          <th v-for="(column, i) in p.columns" :key="i">{{ column }}</th>
        </tr>
      </thead>
      <tbody>
        <tr
          v-for="(cells, index) in p.rows"
          :key="index"
          :class="{ selected: p.selected === index }"
          :aria-selected="p.selected === index"
          tabindex="0"
          @click="emit('select', index)"
          @dblclick="emit('dblclick', index)"
          @keydown="navigate($event, index)"
        >
          <td v-for="(cell, i) in cells" :key="i">{{ cell }}</td>
        </tr>
      </tbody>
    </table>
    <div v-if="!p.rows.length" class="empty-list">
      No items. Use Insert to add one.
    </div>
  </div>
</template>
