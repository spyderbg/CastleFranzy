<script setup lang="ts">
const p = defineProps<{ label: string; value: string }>();
const emit = defineEmits<{ change: [value: string] }>();
function commit(event: FocusEvent) {
  const value = (event.target as HTMLInputElement).value;
  if (value !== p.value) emit("change", value);
}
</script>
<template>
  <label class="field wide"
    ><span>{{ p.label }}</span
    ><input
      :key="p.value"
      :aria-label="p.label"
      :value="p.value"
      @blur="commit"
      @keydown.enter="($event.target as HTMLInputElement).blur()"
  /></label>
</template>
